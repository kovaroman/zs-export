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
}
