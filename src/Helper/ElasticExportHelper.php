<?php

namespace ElasticExport\Helper;

use Plenty\Modules\Category\Contracts\CategoryBranchMarketplaceRepositoryContract;
use Plenty\Modules\Category\Contracts\CategoryBranchRepositoryContract;
use Plenty\Modules\Category\Models\CategoryBranchMarketplace;
use Plenty\Modules\Item\DataLayer\Models\Record;
use Plenty\Modules\Helper\Models\KeyValue;
use Plenty\Modules\Category\Models\CategoryBranch;
use Plenty\Modules\Item\Manufacturer\Contracts\ManufacturerRepositoryContract;
use Plenty\Modules\Item\Manufacturer\Models\Manufacturer;
use Plenty\Modules\Item\Unit\Contracts\UnitNameRepositoryContract;
use Plenty\Modules\Item\Unit\Models\UnitName;
use Plenty\Modules\Item\Attribute\Contracts\AttributeValueNameRepositoryContract;
use Plenty\Modules\Item\Attribute\Models\AttributeValueName;
use Plenty\Modules\Item\Attribute\Models\AttributeName;
use Plenty\Modules\Item\Property\Contracts\PropertyItemNameRepositoryContract;
use Plenty\Modules\Helper\Contracts\UrlBuilderRepositoryContract;
use Plenty\Modules\Category\Contracts\CategoryRepositoryContract;
use Plenty\Modules\Category\Models\Category;
use Plenty\Modules\Item\Property\Contracts\PropertyMarketComponentRepositoryContract;
use Plenty\Modules\Item\Property\Models\PropertyMarketComponent;
use Plenty\Modules\Item\DataLayer\Models\ItemProperty;
use Plenty\Modules\Order\Shipping\Models\DefaultShipping;
use Plenty\Modules\Order\Payment\Method\Contracts\PaymentMethodRepositoryContract;
use Plenty\Modules\Order\Payment\Method\Models\PaymentMethod;
use Plenty\Modules\Item\DefaultShippingCost\Contracts\DefaultShippingCostRepositoryContract;
use Plenty\Modules\Item\Availability\Models\Availability;
use Plenty\Modules\Item\Availability\Models\AvailabilityLanguage;
use Plenty\Plugin\ConfigRepository;
use Plenty\Modules\Order\Shipping\Countries\Contracts\CountryRepositoryContract;
use Plenty\Modules\Order\Shipping\Countries\Models\Country;
use Plenty\Modules\System\Contracts\WebstoreRepositoryContract;
use Plenty\Modules\System\Models\Webstore;
use Plenty\Modules\Item\VariationSku\Contracts\VariationSkuRepositoryContract;
use Plenty\Modules\Item\VariationSku\Models\VariationSku;
use Plenty\Modules\Item\Availability\Contracts\AvailabilityRepositoryContract;
use Plenty\Modules\Item\Attribute\Contracts\AttributeNameRepositoryContract;

/**
 * Class ElasticExportHelper
 * @package ElasticExportHelper\Helper
 */
class ElasticExportHelper
{
    const SHIPPING_COST_TYPE_FLAT = 'flat';
    const SHIPPING_COST_TYPE_CONFIGURATION = 'configuration';

    const IMAGE_POSITION0 = 'position0';
    const IMAGE_FIRST = 'firstImage';

    const REMOVE_HTML_TAGS = 1;
    const KEEP_HTML_TAGS = 0;

    const ITEM_URL_NO = 0;
    const ITEM_URL_YES = 1;

    const TRANSFER_ITEM_AVAILABILITY_NO = 0;
    const TRANSFER_ITEM_AVAILABILITY_YES = 1;

    const TRANSFER_OFFER_PRICE_NO = 0;
    const TRANSFER_OFFER_PRICE_YES = 1;

    const TRANSFER_RRP_YES = 1;
    const TRANSFER_RRP_NO = 0;

    const BARCODE_EAN = 'EAN_13';
    const BARCODE_ISBN = 'ISBN';

    /**
     * CategoryBranchRepositoryContract $categoryBranchRepository
     */
    private $categoryBranchRepository;

    /**
     * UnitNameRepositoryContract $unitNameRepository
     */
    private $unitNameRepository;

	/**
	 * AttributeValueNameRepositoryContract $attributeValueNameRepository
	 */
    private $attributeValueNameRepository;

    /**
     * AttributeNameRepositoryContract $attributeNameRepository
     */
    private $attributeNameRepository;

    /**
     * PropertyItemNameRepositoryContract $propertyItemNameRepository
     */
    private $propertyItemNameRepository;

    /**
     * CategoryBranchMarketplaceRepositoryContract $categoryBranchMarketplaceRepository
     */
    private $categoryBranchMarketplaceRepository;

    /**
     * UrlBuilderRepositoryContract $urlBuilderRepository
     */
    private $urlBuilderRepository;

    /**
     * CategoryRepositoryContract $categoryRepository
     */
    private $categoryRepository;

    /**
     * PropertyMarketComponentRepositoryContract $propertyMarketComponentRepository;s
     */
    private $propertyMarketComponentRepository;

    /**
     * @var PaymentMethodRepositoryContract $paymentMethodRepository
     */
    private $paymentMethodRepository;

    /**
     * @var DefaultShippingCostRepositoryContract $defaultShippingCostRepository
     */
    private $defaultShippingCostRepository;

    /**
     * ConfigRepository $configRepository
     */
    private $configRepository;

    /**
     * CountryRepositoryContract $countryRepository
     */
    private $countryRepository;

    /**
     * WebstoreRepositoryContract $webstoreRepository
     */
    private $webstoreRepository;

    /**
     * VariationSkuRepositoryContract $variationSkuRepository
     */
    private $variationSkuRepository;

    /**
     * AvailabilityRepositoryContract $availabilityRepositoryContract
     */
    private $availabilityRepository;

	/**
	 * ManufacturerRepositoryContract $manufacturerRepository
	 */
	private $manufacturerRepository;

    /**
     * ElasticExportHelper constructor.
     *
     * @param CategoryBranchRepositoryContract $categoryBranchRepository
     * @param UnitNameRepositoryContract $unitNameRepository
     * @param AttributeValueNameRepositoryContract $attributeValueNameRepository
     * @param AttributeNameRepositoryContract $attributeNameRepository
     * @param PropertyItemNameRepositoryContract $propertyItemNameRepository
     * @param CategoryBranchMarketplaceRepositoryContract $categoryBranchMarketplaceRepository
     * @param UrlBuilderRepositoryContract $urlBuilderRepository
     * @param CategoryRepositoryContract $categoryRepository
     * @param PropertyMarketComponentRepositoryContract $propertyMarketComponentRepository
     * @param PaymentMethodRepositoryContract $paymentMethodRepository
     * @param ConfigRepository $configRepository
     * @param CountryRepositoryContract $countryRepository
     * @param WebstoreRepositoryContract $webstoreRepository
     * @param VariationSkuRepositoryContract $variationSkuRepository
     * @param AvailabilityRepositoryContract $availabilityRepository
     */
    public function __construct(CategoryBranchRepositoryContract $categoryBranchRepository,
                                UnitNameRepositoryContract $unitNameRepository,
                                AttributeValueNameRepositoryContract $attributeValueNameRepository,
                                AttributeNameRepositoryContract $attributeNameRepository,
                                PropertyItemNameRepositoryContract $propertyItemNameRepository,
                                CategoryBranchMarketplaceRepositoryContract $categoryBranchMarketplaceRepository,
                                UrlBuilderRepositoryContract $urlBuilderRepository,
                                CategoryRepositoryContract $categoryRepository,
                                PropertyMarketComponentRepositoryContract $propertyMarketComponentRepository,
                        		PaymentMethodRepositoryContract $paymentMethodRepository,
                                DefaultShippingCostRepositoryContract $defaultShippingCostRepository,
                                ConfigRepository $configRepository,
                                CountryRepositoryContract $countryRepository,
                                WebstoreRepositoryContract $webstoreRepository,
                                VariationSkuRepositoryContract $variationSkuRepository,
                                AvailabilityRepositoryContract $availabilityRepository,
								ManufacturerRepositoryContract $manufacturerRepository
    )
    {
        $this->categoryBranchRepository = $categoryBranchRepository;

        $this->unitNameRepository = $unitNameRepository;

        $this->attributeValueNameRepository = $attributeValueNameRepository;

        $this->attributeNameRepository = $attributeNameRepository;

        $this->propertyItemNameRepository = $propertyItemNameRepository;

        $this->categoryBranchMarketplaceRepository = $categoryBranchMarketplaceRepository;

        $this->urlBuilderRepository = $urlBuilderRepository;

        $this->categoryRepository = $categoryRepository;

        $this->propertyMarketComponentRepository = $propertyMarketComponentRepository;

		$this->paymentMethodRepository = $paymentMethodRepository;

        $this->defaultShippingCostRepository = $defaultShippingCostRepository;

        $this->configRepository = $configRepository;

        $this->countryRepository = $countryRepository;

        $this->webstoreRepository = $webstoreRepository;

        $this->variationSkuRepository = $variationSkuRepository;

        $this->availabilityRepository = $availabilityRepository;

		$this->manufacturerRepository = $manufacturerRepository;
    }

    /**
     * Get name.
     *
     * @param  Record    $item
     * @param  KeyValue  $settings
     * @param  int $defaultNameLength
     * @return string
     */
    public function getName(Record $item, KeyValue $settings, int $defaultNameLength = 0):string
	{
		$name = '';

		switch($settings->get('nameId'))
		{
			case 3:
				$name = (string)$item->itemDescription->name3;
                break;

			case 2:
				$name = (string)$item->itemDescription->name2;
                break;

			case 1:
			default:
				$name = (string)$item->itemDescription->name1;
                break;
		}

        return $this->cleanName($name, (int)$settings->get('nameMaxLength') ? (int)$settings->get('nameMaxLength') : (int)$defaultNameLength);
    }

    /**
     * Clean name to a defined length. If maxLength is 0 than named is returned intact.
     * @param  string 	$name
     * @param  int 		$maxLength
     * @return string
     */
    public function cleanName(string $name, int $maxLength = 0):string
    {
        $name = html_entity_decode($name);

        if($maxLength <= 0)
        {
            return $name;
        }

        return substr($name, 0, $maxLength);
    }

    /**
     * Get preview text.
     *
     * @param  Record        $item
     * @param  KeyValue      $settings
     * @param  int           $defaultPreviewTextLength
     * @return string
     */
    public function getPreviewText(Record $item, KeyValue $settings, int $defaultPreviewTextLength = 0):string
    {
        switch($settings->get('previewTextType'))
        {
            case 'itemShortDescription':
                $previewText = $item->itemDescription->shortDescription;
                break;

            case 'technicalData':
                $previewText = $item->itemDescription->technicalData;
                break;

            case 'itemDescriptionAndTechnicalData':
                $previewText = $item->itemDescription->description . ' ' . $item->itemDescription->technicalData;
                break;

            case 'itemDescription':
                $previewText = $item->itemDescription->description;
                break;

            case 'dontTransfer':
            default:
                $previewText = '';
                break;
        }

        $previewTextLength = $settings->get('previewTextMaxLength') ? $settings->get('previewTextMaxLength') : $defaultPreviewTextLength;

        if($settings->get('previewTextRemoveHtmlTags') == self::REMOVE_HTML_TAGS)
        {
            $previewText = strip_tags($previewText, str_replace([',', ' '], '', $settings->get('previewTextAllowHtmlTags')));
        }

        if($previewTextLength <= 0)
        {
            return $previewText;
        }

        return substr($previewText, 0, $previewTextLength);
    }

    /**
     * Get description.
     *
     * @param  Record        $item
     * @param  KeyValue      $settings
     * @param  int           $defaultDescriptionLength
     * @return string
     */
    public function getDescription(Record $item, KeyValue $settings, int $defaultDescriptionLength = 0):string
    {
        switch($settings->get('descriptionType'))
        {
            case 'itemShortDescription':
                $description = $item->itemDescription->shortDescription;
                break;

            case 'technicalData':
                $description = $item->itemDescription->technicalData;
                break;

            case 'itemDescriptionAndTechnicalData':
                $description = $item->itemDescription->description . ' ' . $item->itemDescription->technicalData;
                break;

            case 'itemDescription':
            default:
                $description = $item->itemDescription->description;
                break;
        }

        $descriptionLength = $settings->get('descriptionMaxLength') ? $settings->get('descriptionMaxLength') : $defaultDescriptionLength;

        if($settings->get('descriptionRemoveHtmlTags') == self::REMOVE_HTML_TAGS)
        {
            $description = strip_tags($description, str_replace([',', ' '], '', $settings->get('previewTextAllowHtmlTags')));
        }

        $description = html_entity_decode($description);

        if($descriptionLength <= 0)
        {
            return $description;
        }

        return substr($description, 0, $descriptionLength);
    }

    /**
	 * Get variation availability days.
	 * @param  Record   $item
	 * @param  KeyValue $settings
	 * @param  bool 	$returnAvailabilityName = true
	 * @return string
	 */
	public function getAvailability(Record $item, KeyValue $settings, bool $returnAvailabilityName = true):string
	{
        if($settings->get('transferItemAvailability') == self::TRANSFER_ITEM_AVAILABILITY_YES)
		{
            $availabilityIdString = 'itemAvailability' . $item->variationBase->availability;

		    return (string)$settings->get($availabilityIdString);
		}

        $availability = $this->availabilityRepository->findAvailability($item->variationBase->availability < 0 ? 10 : (int) $item->variationBase->availability);

        if($availability instanceof Availability)
        {
            $name = $this->getAvailabilityName($availability, $settings->get('lang'));

            if($returnAvailabilityName && strlen($name) > 0)
            {
                return (string) $name;
            }
            elseif(!$returnAvailabilityName && $availability->averageDays > 0)
            {
                return (string) $availability->averageDays;
            }
        }

		return '';
	}

    /**
     * Get availability name for a vigen availability and lang.
     * @param Availability $availability
     * @param string $language
     * @return string
     */
    private function getAvailabilityName(Availability $availability, string $language):string
    {
        foreach($availability->languages as $availabilityLanguage)
        {
            if($availabilityLanguage->language == $language)
            {
                return (string)$availabilityLanguage->name;
            }
        }

        return '';
    }

    /**
     * Get the item URL.
     * @param  Record $item
     * @param  KeyValue $settings
     * @param  bool $addReferrer = true  Choose if referrer id should be added as parameter.
     * @param  bool $useIntReferrer = false Choose if referrer id should be used as integer.
     * @param  bool $useHttps = true Choose if https protocol should be used.
     * @return string Item url.
     */
    public function getUrl(Record $item, KeyValue $settings, bool $addReferrer = true, bool $useIntReferrer = false, bool $useHttps = true):string
	{
        if($settings->get('itemUrl') == self::ITEM_URL_NO)
        {
            return '';
        }

		$urlParams = [];

        $link = $this->urlBuilderRepository->getItemUrl($item->itemBase->id, $settings->get('plentyId'), $item->itemDescription->urlContent, $settings->get('lang') ? $settings->get('lang') : 'de');

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
     * Get category branch for a custom category id.
     * @param  int $categoryId
     * @param  string $lang
     * @param  int $plentyId
     * @param  string $separator = ' > '
     * @return string
     */
    public function getCategory(int $categoryId, string $lang, int $plentyId, string $separator = ' > '):string
	{
        $categoryBranchList = array();

        if($categoryId > 0)
        {
            $categoryBranch = $this->categoryBranchRepository->find($categoryId);

            if(!is_null($categoryBranch) && $categoryBranch instanceof CategoryBranch)
            {
                $category1 = $this->categoryRepository->get($categoryBranch->category1Id, $lang);
                $category2 = $this->categoryRepository->get($categoryBranch->category2Id, $lang);
                $category3 = $this->categoryRepository->get($categoryBranch->category3Id, $lang);
                $category4 = $this->categoryRepository->get($categoryBranch->category4Id, $lang);
                $category5 = $this->categoryRepository->get($categoryBranch->category5Id, $lang);
                $category6 = $this->categoryRepository->get($categoryBranch->category6Id, $lang);

                if($category1 instanceof Category)
                {
                    $category1Details = $category1->details[0];
                    $categoryBranchList[] = $category1Details->name;
                }
                if($category2 instanceof Category)
                {
                    $category2Details = $category2->details[0];
                    $categoryBranchList[] = $category2Details->name;
                }
                if($category3 instanceof Category)
                {
                    $category3Details = $category3->details[0];
                    $categoryBranchList[] = $category3Details->name;
                }
                if($category4 instanceof Category)
                {
                    $category4Details = $category4->details[0];
                    $categoryBranchList[] = $category4Details->name;
                }
                if($category5 instanceof Category)
                {
                    $category5Details = $category5->details[0];
                    $categoryBranchList[] = $category5Details->name;
                }
                if($category6 instanceof Category)
                {
                    $category6Details = $category6->details[0];
                    $categoryBranchList[] = $category6Details->name;
                }
                if(is_array($categoryBranchList) && count($categoryBranchList) > 0)
                {
                    return implode($separator, $categoryBranchList);
                }
            }
        }

        return '';
	}

    public function getCategoryBranch(Record $item, KeyValue $settings, int $categoryLevel):string
    {
		if($item->variationStandardCategory->categoryId <= 0)
		{
			return '';
		}

        $categoryBranch = $this->categoryBranchRepository->find($item->variationStandardCategory->categoryId);
        $category = null;
        $lang = $settings->get('lang') ? $settings->get('lang') : 'de';

        if(!is_null($categoryBranch) && $categoryBranch instanceof CategoryBranch)
        {
            switch($categoryLevel)
            {
                case 1:
                    $category = $this->categoryRepository->get($categoryBranch->category1Id, $lang);
                    break;

                case 2:
                    $category = $this->categoryRepository->get($categoryBranch->category2Id, $lang);
                    break;

                case 3:
                    $category = $this->categoryRepository->get($categoryBranch->category3Id, $lang);
                    break;

                case 4:
                    $category = $this->categoryRepository->get($categoryBranch->category4Id, $lang);
                    break;

                case 5:
                    $category = $this->categoryRepository->get($categoryBranch->category5Id, $lang);
                    break;

                case 6:
                    $category = $this->categoryRepository->get($categoryBranch->category6Id, $lang);
                    break;
            }
        }

        if($category instanceof Category)
        {
            foreach($category->details as $categoryDetails)
            {
                if($categoryDetails->lang == $lang)
                {
                    return (string)$categoryDetails->name;
                }
            }
        }

        return '';
    }

	/**
	 * Get category branch marketplace for a custom branch id.
	 * @param  int $categoryhId
	 * @param  int $plentyId
	 * @param  int $marketplaceId
	 * @param  string $marketplaceSubId
	 * @return string
	 */
	public function getCategoryMarketplace(int $categoryhId, int $plentyId, int $marketplaceId, float $marketplaceSubId = 0.0):string
	{
        $webstoreId = $this->getWebstoreId($plentyId);
		$categoryBranchMarketplace = $this->categoryBranchMarketplaceRepository->findCategoryBranchMarketplace($categoryhId, $webstoreId, $marketplaceId, $marketplaceSubId);

		if($categoryBranchMarketplace instanceof CategoryBranchMarketplace)
		{
			return (string)$categoryBranchMarketplace->plenty_category_branch_marketplace_value1;
		}

		return '';
	}

    /**
     * Get shipping cost.
     * @param  Record   $item
     * @param  KeyValue $settings
     * @param  int|null  $mobId
     * @return float|null
     */
    public function getShippingCost(Record $item, KeyValue $settings, int $mopId = null)
    {
        if($settings->get('shippingCostType') == self::SHIPPING_COST_TYPE_FLAT)
        {
            return (float) $settings->get('shippingCostFlat');
        }

        if($settings->get('shippingCostType') == self::SHIPPING_COST_TYPE_CONFIGURATION)
        {
            $defaultShipping = $this->getDefaultShipping($settings);

            if( $defaultShipping instanceof DefaultShipping &&
                $defaultShipping->shippingDestinationId)
            {
                if(!is_null($mopId) && $mopId == $defaultShipping->paymentMethod2)
                {
                    $paymentMethodId = $defaultShipping->paymentMethod2;
                    return $this->calculateShippingCost($item->itemBase->id, $defaultShipping->shippingDestinationId, $defaultShipping->referrerId, $paymentMethodId);
                }
                if(!is_null($mopId) && $mopId == $defaultShipping->paymentMethod3)
                {
                    $paymentMethodId = $defaultShipping->paymentMethod3;
                    return $this->calculateShippingCost($item->itemBase->id, $defaultShipping->shippingDestinationId, $defaultShipping->referrerId, $paymentMethodId);
                }
                $paymentMethodId = $defaultShipping->paymentMethod2;

                // 0 - is always "payment in advance" so we use always the second and third payment methods from the default shipping
                if($settings->get('shippingCostMethodOfPayment') == 2)
                {
                    $paymentMethodId = $defaultShipping->paymentMethod3;
                }
                if(!is_null($mopId) && $mopId >= 0)
                {
                    if($mopId == $paymentMethodId)
                    {
                        return $this->calculateShippingCost($item->itemBase->id, $defaultShipping->shippingDestinationId, $defaultShipping->referrerId, $paymentMethodId);
                    }
                }
                elseif(is_null($mopId))
                {
                    return $this->calculateShippingCost($item->itemBase->id, $defaultShipping->shippingDestinationId, $defaultShipping->referrerId, $paymentMethodId);
                }
            }
        }
        return null;
    }

    /**
     * Calculate default shipping cost.
     * @param int $itemId
     * @param int $shippingDestinationId
     * @param float $referrerId
     * @param int $paymentMethodId
     * @return float|null
     */
    public function calculateShippingCost(int $itemId, int $shippingDestinationId, float $referrerId, int $paymentMethodId)
    {
        return $this->defaultShippingCostRepository->findShippingCost($itemId, $referrerId, $shippingDestinationId, $paymentMethodId);
    }

    /**
     * returns the price of the given variation
     * @param  Record   $item
     * @return float|null
     */
    public function getPrice(Record $item)
    {
            return $item->variationRetailPrice->price;
    }

    /**
     * returns the recommendedRetailPrice of the given variation if transferRrp is set
     * @param Record $item
     * @param KeyValue $settings
     * @return float
     */
    public function getRecommendedRetailPrice(Record $item, KeyValue $settings):float
    {
        if($settings->get('transferRrp') == self::TRANSFER_RRP_YES)
        {
            return (float)$item->variationRecommendedRetailPrice->price;
        }

        return 0.00;
    }

    /**
     * returns the specialOfferPrice of the given variation if transferOfferPrice is set
     * @param Record $item
     * @param KeyValue $settings
     * @return float
     */
    public function getSpecialPrice(Record $item, KeyValue $settings):float
    {
        if($settings->get('transferOfferPrice') == self::TRANSFER_OFFER_PRICE_YES)
        {
            return (float)$item->variationSpecialOfferRetailPrice->retailPrice;
        }

        return 0.00;
    }

    /**
     * Get the attributeNames
     * @param Record   $item
     * @param KeyValue $settings
     * @return string
     */
    public function getAttributeName(Record $item, KeyValue $settings):string
    {
        $values = [];

        if($item->variationBase->attributeValueSetId)
        {
            foreach($item->variationAttributeValueList as $attribute)
            {
                $attributeName = $this->attributeNameRepository->findOne($attribute->attributeId, $settings->get('lang') ? $settings->get('lang') : 'de');
                if($attributeName instanceof AttributeName)
                {
                    $values[] = $attributeName->name;
                }
            }
        }

        return implode('|', $values);
    }

    /**
     * Get the attribute value set short frontend name. Ex. blue, XL
     * @param  Record   $item
     * @param  KeyValue $settings
     * @param  string $delimiter
     * @param  array $attributeNameCombination
     * @return string
     */
    public function getAttributeValueSetShortFrontendName(Record $item, KeyValue $settings, string $delimiter = ', ', array $attributeNameCombination = null):string
    {
        $values = [];
        $unsortedValues = [];

        if($item->variationBase->attributeValueSetId)
        {
            $i = 0;
            foreach($item->variationAttributeValueList as $attribute)
            {
                $attributeValueName = $this->attributeValueNameRepository->findOne($attribute->attributeValueId, $settings->get('lang') ? $settings->get('lang') : 'de');

                if($attributeValueName instanceof AttributeValueName)
                {
                    $unsortedValues[$attribute->attributeId] = $attributeValueName->name;
                    $i++;
                }
            }

            /**
             * sort the attribute value names depending on the order of the $attributeNameCombination
             */
            if(is_array($attributeNameCombination) && count($attributeNameCombination) > 0)
            {
                $j = 0;
                while($i > 0)
                {
                    $values[] = $unsortedValues[$attributeNameCombination[$j]];
                    $j++;
                    $i--;
                }
            }
            else
            {
                $values = $unsortedValues;
            }
        }

        return implode($delimiter, $values);
    }

    /**
     * Get base price.
     * @param  Record   $item
     * @param  KeyValue $settings
     * @param  string   $separator	= '/'
     * @param  bool     $compact    = false
     * @param  bool     $dotPrice   = false
     * @param  string   $currency   = ''
     * @param  float    $price      = 0.0
     * @param  bool     $addUnit    = true
     * @return string
     */
    public function getBasePrice(
        Record $item,
        KeyValue $settings,
        string $separator = '/',
        bool $compact = false,
        bool $dotPrice = false,
        string $currency = '',
        float $price = 0.0,
        bool $addUnit = true
    ):string
	{
        $currency = strlen($currency) ? $currency : $this->getDefaultCurrency();
		$price = $price > 0 ? $price : (float) $item->variationRetailPrice->price;
        $lot = (int) $item->variationBase->content;
        $unitLang = $this->unitNameRepository->findByUnitId((int) $item->variationBase->unitId);

        if($unitLang instanceof UnitName)
        {
            $unitShortcut = $unitLang->unit->unitOfMeasurement;
            $unitName = $unitLang->name;
        }
        else
        {
            $unitShortcut = '';
            $unitName = '';
        }

		$basePriceDetails = $this->getBasePriceDetails($lot, $price, $unitShortcut);

		if((float) $basePriceDetails['price'] <= 0 || ((int) $basePriceDetails['lot'] <= 1 && $basePriceDetails['unit'] == 'C62'))
		{
			return '';
		}

		if ($dotPrice == true)
		{
			$basePriceDetails['price'] = number_format($basePriceDetails['price'], 2, '.', '');
		}
		else
		{
			$basePriceDetails['price'] = number_format($basePriceDetails['price'], 2, ',', '');
		}

		if ($addUnit == true)
        {
            if ($compact == true)
            {
                return	'(' . (string) $basePriceDetails['price'] . $currency . $separator . (string) $basePriceDetails['lot'] . $unitShortcut . ')';
            }
            else
            {
                return	(string) $basePriceDetails['price'] . ' ' . $currency . $separator . (string) $basePriceDetails['lot'] . ' ' . $unitName;
            }
        }
        else
        {
            return	(string) $basePriceDetails['price'];
        }
	}

	/**
	 * Get base price.
	 *
	 * @param  Record   $item
	 * @param  KeyValue $settings
	 * @return array
	 */
	public function getBasePriceList(Record $item, KeyValue $settings):array
	{
		$price = (float)$item->variationRetailPrice->price;
		$lot = (int)$item->variationBase->content;
		$unitLang = $this->unitNameRepository->findByUnitId((int)$item->variationBase->unitId);

		if($unitLang instanceof UnitName)
		{
            $unitShortcut = $unitLang->unit->unitOfMeasurement;
			$unitName = $unitLang->name;
		}
		else
		{
            $unitShortcut = '';
			$unitName = '';
		}

		$basePriceDetails = $this->getBasePriceDetails($lot, $price, $unitShortcut);

		$basePriceDetails['price'] = number_format($basePriceDetails['price'], 2, '.', '');

        return [
        	'lot' => (int)$basePriceDetails['lot'],
			'price' => (float)$basePriceDetails['price'],
			'unit' => (string)$unitName
		];
	}

    /**
     * Get base price unit as short cut.
     *
     * @param  Record   $item
     * @param  KeyValue $settings
     * @return string
     */
    public function getBasePriceDetailUnit(Record $item, KeyValue $settings):string
    {
        $unitLang = $this->unitNameRepository->findByUnitId((int) $item->variationBase->unitId);

		if($unitLang instanceof UnitName)
		{
            $unitShortcut = $unitLang->unit->unitOfMeasurement;
		}
		else
		{
            $unitShortcut = '';
		}

        return $unitShortcut;
    }

    /**
     * Get main image.
     * @param  Record   $item
     * @param  KeyValue $settings
	 * @param  string 	$imageType
     * @return string
     */
    public function getMainImage(Record $item, KeyValue $settings, string $imageType = 'normal'):string
    {
        foreach($item->variationImageList as $image)
        {
            if($settings->get('imagePosition') == self::IMAGE_FIRST)
            {
                return $this->urlBuilderRepository->getImageUrl($image->path, $settings->get('plentyId'), $imageType, $image->fileType, $image->type == 'external');
            }
            elseif($settings->get('imagePosition')== self::IMAGE_POSITION0 && $image->position == 0)
            {
                return $this->urlBuilderRepository->getImageUrl($image->path, $settings->get('plentyId'), $imageType, $image->fileType, $image->type == 'external');
            }
        }

        return '';
    }

    /**
     * Get list of images for current item.
     * @param Record $item
     * @param KeyValue $settings
     * @param string $imageType = 'normal'
     * @return array
     */
    public function getImageList(Record $item, KeyValue $settings, string $imageType = 'normal'):array
    {
        $list = [];

        foreach($item->variationImageList as $image)
        {
            $list[] = $this->urlBuilderRepository->getImageUrl($image->path, $settings->get('plentyId'), $imageType, $image->fileType, $image->type == 'external');
        }

        return $list;
    }

    /**
     * Get item character value by backend name.
     * @param  Record $item
     * @param KeyValue $settings
     * @param  string $backendName
     * @return string
     */
    public function getItemCharacterByBackendName(Record $item, KeyValue $settings, string $backendName):string
    {
        foreach($item->itemPropertyList as $itemProperty)
        {
            $propertyItemName = $this->propertyItemNameRepository->findOne($itemProperty->propertyId, $settings->get('lang')? $settings->get('lang') : 'de');

            if($propertyItemName->name == $backendName)
            {
                return (string) $itemProperty->propertyValue;
            }
        }

        return '';
    }

    /**
     * Get item characters that match referrer from settings and a given component id.
     * @param  Record   $item
     * @param  float    $marketId
     * @param  int     $componentId  = null
     * @return array
     */
    public function getItemCharactersByComponent(Record $item, float $marketId, int $componentId = null):array
    {
        $propertyList = $item->itemPropertyList;
        $propertyMarketComponents = $this->propertyMarketComponentRepository->getPropertyMarketComponents($marketId, $componentId);
        $list = array();

        foreach($propertyList as $property)
		{
            foreach($propertyMarketComponents as $propertyMarketComponent)
            {
                if($propertyMarketComponent instanceof PropertyMarketComponent && $propertyMarketComponent->propertyItemId == $property->propertyId)
                {
                    $list[] = [
                        'itemCharacterId' 	 => $property->itemPropertyId,
                        'characterId' 		 => $property->propertyId,
                        'characterValue' 	 => $property->propertyValue,
                        'characterValueType' => $property->propertyValueType,
                        'characterItemId' 	 => $propertyMarketComponent->propertyItemId,
                        'componentId' 		 => $propertyMarketComponent->componentId,
                        'referrerId' 		 => $propertyMarketComponent->marketReference,
                        'externalComponent'  => $propertyMarketComponent->externalComponent,
					];
                }
            }
		}

		return $list;
    }

    /**
     * Get barcode by a given type.
     * @param  Record   $item
     * @param  string   $barcodeType
     * @return string
     */
    public function getBarcodeByType(Record $item, string $barcodeType):string
    {
        foreach($item->variationBarcodeList as $variationBarcode)
        {
            if($variationBarcode->barcodeType == $barcodeType || $barcodeType == 'FirstBarcode')
            {
                return (string) $variationBarcode->code;
            }
        }

        return '';
    }

    /**
     * Get base price details.
     * @param  int    $lot
     * @param  float  $price
     * @param  string $unit
     * @return array
     */
    public function getBasePriceDetails(int $lot, float $price, string $unit):array
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

		return [
			'lot' => (int) $basePriceLot,
			'price' => (float) $price * $factor * $endLot,
			'unit' => (string) $basePriceUnit
		];
    }

    /**
     * Get default currency from configuration.
     * @return string
     */
    public function getDefaultCurrency():string
    {
        $config = [];

		// TODO load config
//		$config = $this->getConfig('cfgCurrency');

        if(is_array($config) && is_string($config['cfgCurrency']))
        {
            return $config['cfgCurrency'];
        }

        return 'EUR';
    }

    /**
     * Get list of payment methods.
     * @param KeyValue $settings
     * @return array
     */
    public function getPaymentMethods(KeyValue $settings):array
    {
        $paymentMethods = $this->paymentMethodRepository->getPaymentMethods($settings->get('destination'), $settings->get('plentyId'), $settings->get('lang'));

        $list = array();

        foreach($paymentMethods as $paymentMethod)
        {
            $list[$paymentMethod->id] = $paymentMethod;
        }

        return $list;
    }

    /**
	 * Get the default shipping.
	 * @param  KeyValue $settings
	 * @return DefaultShipping|null
	 */
	public function getDefaultShipping(KeyValue $settings):DefaultShipping
	{
        $defaultShippingProfiles = $this->getConfig('plenty.order.shipping.default_shipping');

        foreach($defaultShippingProfiles as $defaultShippingProfile)
        {
            if($defaultShippingProfile instanceof DefaultShipping && $defaultShippingProfile->id == $settings->get('shippingCostConfiguration'))
            {
                return $defaultShippingProfile;
            }
        }

        return null;
	}

    /**
     * Get custom configuration.
     * @param  string $key
     * @param  mixed  $default = null
     * @return array
     */
    public function getConfig(string $key, $default = null)
    {
        return $this->configRepository->get($key, $default);
    }

    /**
     * @param KeyValue $settings
     * @param string $isoCodeType
     * @return string
     */
    public function getCountry(KeyValue $settings, string $isoCodeType):string
    {
        $country = $this->countryRepository->findIsoCode($settings->get('destination'), $isoCodeType);

        return $country;
    }

    /**
     * @param int $plentyId
     * @return int
     */
    public function getWebstoreId(int $plentyId):int
    {
        $webstore = $this->webstoreRepository->findByPlentyId($plentyId);

        if($webstore instanceof Webstore)
        {
            $webstoreId = $webstore->id;

			if(!is_null($webstoreId))
			{
				return $webstoreId;
			}
        }

        return 0;
    }

    /**
     * @param Record $item
     * @param int $marketId
     * @param null|string $sku
     * @param int $accountId
     * @param bool $setLastExportedTimestamp
     * @return string
     */
    public function generateSku(Record $item,
								int $marketId,
								string $sku = null,
								int $accountId = 0,
								bool $setLastExportedTimestamp = true
	):string
    {
        $sku = $this->variationSkuRepository->generateSku($item->variationBase->id,
														  $marketId,
														  $accountId,
														  $sku,
														  $setLastExportedTimestamp);

        return $sku;
    }

    /**
	 * Selects the external manufacturer name by id.
	 *
	 * @param int $manufacturerId
	 * @return string
	 */
    public function getExternalManufacturerName(int $manufacturerId):string
	{
		if($manufacturerId > 0)
		{
			$manufacturer = $this->manufacturerRepository->findById($manufacturerId);

			if($manufacturer instanceof Manufacturer)
			{
				if(strlen($manufacturer->externalName))
				{
					return $manufacturer->externalName;
				}
				elseif(strlen($manufacturer->name))
				{
					return $manufacturer->name;
				}
			}
		}

		return '';
	}
}
