<?hh // strict

namespace ElasticExport\Helper;


use Plenty\Modules\Item\DataLayer\Models\Record;
use Plenty\Modules\Helper\Models\KeyValue;

/**
 * Class ElasticExportHelper
 * @package ElasticExportHelper\Helper
 */
class ElasticExportHelper
{
    const SHIPPING_COST_TYPE_FLAT = 'flat';
    const SHIPPING_COST_TYPE_CONFIGURATION = 'configuration';

    /**
     * Get name.
     *
     * @param  Record    $item
     * @param  KeyValue  $settings
     * @return string
     */
    public function getName(Record $item, KeyValue $settings):string
	{
		if($settings->get('nameId'))
		{
			switch($settings->get('nameId'))
			{
				case 3:
					return $item->itemDescription->name3;

				case 2:
					return $item->itemDescription->name2;

				case 1:
				default:
					return $item->itemDescription->name1;
			}
        }

        return $item->itemDescription->name1;
    }

    /**
     * Get description.
     *
     * @param  Record        $item
     * @param  KeyValue      $settings
     * @param  int           $defaultDescriptionLength
     * @return string
     */
    public function getDescription(Record $item, KeyValue $settings, int $defaultDescriptionLength):string
    {
        if($settings->get('descriptionLength'))
        {
            $descriptionLength = $settings->get('descriptionLength');
        }
        else
        {
            $descriptionLength = $defaultDescriptionLength;
        }

        if($settings->get('descriptionType'))
        {
            switch($settings->get('descriptionType'))
            {
                case 'shortDescription':
                    return substr(strip_tags($item->itemDescription->shortDescription), 0, $descriptionLength);

                case 'technicalData':
                    return substr(strip_tags($item->itemDescription->technicalData), 0, $descriptionLength);

                case 'description':
                default:
                    return substr(strip_tags($item->itemDescription->description), 0, $descriptionLength);
            }
        }

        return substr(strip_tags($item->itemDescription->description), 0, $descriptionLength);
    }

    /**
	 * Get variation availability days.
	 * @param  Record   $item
	 * @param  KeyValue $settings
	 * @return int
	 */
	public function getAvailability(Record $item, KeyValue $settings):int
	{
		if($settings->get('transferItemAvailability'))
		{
            $availabilityIdString = 'itemAvailability' . $item->variationBase->availability;

		    return $settings->get($availabilityIdString);
		}

		// TODO match variation avialibility Id with the system configuration

		return 1;
	}

    /**
     * Get the item URL.
     * @param  {[type]} Record    $item           [description]
     * @param  {[type]} KeyValue  $settings       [description]
     * @param  {[type]} bool      $addReferrer    =             true  Choose if referrer id should be added as parameter.
     * @param  {[type]} bool      $useIntReferrer =             false Choos if referrer id should be used as integer.
     * @return {[type]}           Item url.
     */
    public function getUrl(Record $item, KeyValue $settings, bool $addReferrer = true, bool $useIntReferrer = false):string
	{
		$urlParams = [];

        $link = ''; // TODO UrlBuilder get url_content

		if($addReferrer && $settings->get('referrerId'))
		{
            $urlParams[] = 'ReferrerID=' . ($useIntReferrer ? (int) $settings->get('referrerId') : $settings->get('referrerId'));
		}

		if(strlen($settings->get('urlParam')))
		{
			$urlParams[] = $settings->get('urlParam');
		}

		if (is_array($urlParams) && count($urlParams) > 0)
		{
			$link .= '?' . implode('&', $urlParams);
		}

		return $link;
	}

    /**
     * Get category.
     * @param  Record   $item
     * @param  KeyValue $settings
     * @param  string   $separator
     * @return string
     */
    public function getCategory(Record $item, KeyValue $settings, string $separator = ' > '):string
	{
        return 'Category: ' . $item->variationStandardCategory->categoryId;

        /* TODO
        $categoryData = ItemDataLayerHelperCategory::getCategoryById($item->variationStandardCategory->categoryId);


		if($categoryData->getBranchName())
		{
			return implode($separator, $categoryData->getBranchName());
		}

		return '';
        */
	}

    /**
     * Get shipping cost.
     * @param  Record   $item
     * @param  KeyValue $settings
     * @return float
     */
    public function getShippingCost(Record $item, KeyValue $settings):float
    {
        if($settings->get('shippingCostType') == self::SHIPPING_COST_TYPE_FLAT)
        {
            return $settings->get('shippingCostFlat');
        }

        switch($settings->get('shippingCostConfiguration'))
        {
            case 1:
                // TODO use new shipping cost helper

            case 2:
                // TODO use new shipping cost helper
        }


        return 1.2;
    }

    public function getBasePrice(Record $item, KeyValue $settings, string $separator = '/', bool $compact = false, bool $dotPrice = false, string $singleValueReturn = '', string $currency = ''):string
	{
		$price = number_format($item->variationRetailPrice->price, 2, '.', '');
        $content = $item->variationBase->content;
        $unit = 'KGM'; // TODO ItemDataLayerHelperUnit::getUnitById($item->variationBase->unitId);

		$basePrice = $this->getBasePriceDetails($content, $price, $unit);

        return '100 Euro/KG';
	}


    private function getBasePriceDetails(int $lot, float $price, string $unit):array<mixed>
    {
        $lot = $lot == 0 ? 1 : $lot; // TODO  PlentyStringUtils::numberFormatLot($lot, true);
		$basePrice = 0;
		$basePriceLot = 1;
        $unit = strlen($unit) ? $unit : 'C62';
        $basePriceUnit = $unit;

		$factor = 1.0;

		if($unit == 'LTR' || $unit == 'KGM')
		{
			$basePriceLot = 1;
		}
		elseif($unit == 'GRM' || $unit == 'MLT')
		{
			if($lot <= 250)
			{
				$basePriceLot = 100;
			}
			else
			{
				$factor = 1000.0;
				$basePriceLot = 1;
				$basePriceUnit = $unit =='GRM' ? 'KGM' : 'LTR';
			}
		}
		else
		{
			$basePriceLot = 1;
		}

		$endLot = ($basePriceLot/$lot);

		return array($basePriceLot, $price * $factor * $endLot, $basePriceUnit);
    }
}
